import fs from 'fs/promises';
import cp from 'child_process';
import util from 'util';
import dotenv from 'dotenv';
import express from 'express';
import helmet from 'helmet';
import cors from 'cors';
import { coerce, number, object, string, validate } from 'superstruct';
import { withTempDir } from './utils.js';

dotenv.config();
const exec = util.promisify(cp.exec);
const app = express();
app.use(cors());
app.use(helmet());

const CLI_COMMAND = 'npx cli prove';
const PROOF_FILENAME = 'proof';
const INPUT_FILENAME = 'primary-input';

const provingKey = process.env.PROVING_KEY;

// const NumberFromString = coerce(number(), string(), (s) => parseInt(s, 10));
const FloatFromString = coerce(number(), string(), (s) =>
  parseFloat(parseFloat(s).toFixed(2))
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
    const provingKeyPath = `${pwd}/${INPUT_FILENAME}`;
    const paths = {
      'proof-path': proofPath,
      'primary-input-path': primaryInputPath,
      'proving-key': provingKeyPath,
    };
    await fs.writeFile(provingKeyPath, Buffer.from(provingKey, 'hex'));
    const command = `${CLI_COMMAND} ${argsFromObject({ ...coords, ...paths })}`;
    await fs.writeFile('cmd.log', command);
    const { stdout: log, stderr: errors } = await exec(command);
    console.log('========================================');
    console.log('log', log);
    console.log('========================================');
    console.log('errors', errors);
    console.log('========================================');
    console.log('Done.');
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
    const proof = await generateProof(query);
    res.json({ hex: proof });
  }
});

app.listen(8123, () => console.log('Listening on 8123...'));
