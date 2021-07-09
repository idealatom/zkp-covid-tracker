import cp from 'child_process';
import util from 'util';
import express from 'express';
import helmet from 'helmet';
import cors from 'cors';
import { coerce, number, object, string, validate } from 'superstruct';

const exec = util.promisify(cp.exec);
const app = express();
app.use(cors());
app.use(helmet());

const CLI_COMMAND = 'npx cli';

// const NumberFromString = coerce(number(), string(), (s) => parseInt(s, 10));
const FloatFromString = coerce(number(), string(), (s) => parseFloat(s));

const Query = object({
  minLat: FloatFromString,
  maxLat: FloatFromString,
  minLng: FloatFromString,
  maxLng: FloatFromString,
  posLat: FloatFromString,
  posLng: FloatFromString,
});

const argsFromObject = (object) =>
  Object.keys(object).reduce((acc, x) => `${acc} -${x} ${object[x]}`, '');

const generateProof = async (coords) => {
  const { stdout } = await exec(`${CLI_COMMAND} ${argsFromObject(coords)}`);
  // `${CLI_COMMAND} ${argsFromObject(coords)} | xxd -p | tr -d '\n'`
  return stdout;
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
