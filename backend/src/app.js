import fs from 'fs/promises';
import cp from 'child_process';
import util from 'util';
import express from 'express';
import helmet from 'helmet';
import cors from 'cors';
import { coerce, number, object, string, validate } from 'superstruct';
import { withTempDir } from './utils.js';

const exec = util.promisify(cp.exec);
const app = express();
app.use(cors());
app.use(helmet());

const CLI_COMMAND = './assets/cli prove';
const PROOF_FILENAME = 'proof';
const INPUT_FILENAME = 'primary-input';
const PK_PATH = './assets/proving.key';

const FloatFromString = coerce(number(), string(), (s) =>
  parseFloat(parseFloat(s).toFixed(6))
);

const Query = object({
  minLat: FloatFromString,
  maxLat: FloatFromString,
  minLng: FloatFromString,
  maxLng: FloatFromString,
  posLat: FloatFromString,
  posLng: FloatFromString,
});

const argsFromObject = (object) =>
  Object.keys(object).reduce((acc, x) => `${acc} --${x} ${object[x]}`, '');

const generateProof = async (coords) => {
  console.log('Generating proof...');
  return await withTempDir('cli-', async (pwd) => {
    const proofPath = `${pwd}/${PROOF_FILENAME}`;
    const primaryInputPath = `${pwd}/${INPUT_FILENAME}`;
    const paths = {
      'proof-path': proofPath,
      'primary-input-path': primaryInputPath,
      'proving-key': PK_PATH,
    };
    const command = `${CLI_COMMAND} ${argsFromObject({ ...coords, ...paths })}`;
    await fs.writeFile('cmd.log', command);
    const { stdout: log, stderr: errors } = await exec(command);
    console.log(log);
    console.log('Done.');
    await fs.writeFile('output.log', log);
    await fs.writeFile('errors.log', errors);
    const proof = await fs.readFile(proofPath);
    const input = await fs.readFile(primaryInputPath);
    const hex = `${proof.toString('hex')}${input.toString('hex')}`;
    console.log('Result in hex:', hex);
    return hex;
  });
};

app.get('/proof/generate', async (req, res) => {
  const [error, query] = validate(req.query, Query, { coerce: true });

  if (error) {
    res.json({ success: false, errors: error.failures() });
  } else {
    try {
      const proof = await generateProof(query);
      res.json({ hex: proof });
    } catch (e) {
      console.log('Exit code:', e.code);
      if (e.code === 200) {
        console.log('Blueprint is not satisfied.');
        res.status(406).json({ error: 'Blueprint is not satisfied' });
      } else {
        throw e;
      }
    }
  }
});

app.listen(8123, () => console.log('Listening on 8123...'));
