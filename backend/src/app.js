import express from 'express';
import helmet from 'helmet';
import fs from 'fs/promises';
import cp from 'child_process';
import util from 'util';
import { coerce, number, object, string, validate } from 'superstruct';
import { customAlphabet } from 'nanoid';

const nanoid = customAlphabet('0123456789abcdef', 8);
const exec = util.promisify(cp.exec);
const app = express();
app.use(helmet());

const CLI_COMMAND = 'npx cli';
const PWD = '/tmp';

const NumberFromString = coerce(number(), string(), (s) => parseInt(s, 10));

const Query = object({
  latMin: NumberFromString,
  latMax: NumberFromString,
  lngMin: NumberFromString,
  lngMax: NumberFromString,
  posLat: NumberFromString,
  posLng: NumberFromString,
});

const argsFromObject = (object) =>
  Object.keys(object).reduce((acc, x) => `${acc} -${x} ${object[x]}`, '');

const generateProof = async (coords) => {
  const name = `proof_${nanoid()}`;
  await exec(`${CLI_COMMAND} ${argsFromObject(coords)} > ${PWD}/${name}`);
  return name;
};

const getFile = (name) => fs.readFile(`${PWD}/${name}`);
const deleteFile = (name) => fs.unlink(`${PWD}/${name}`);

app.get('/proof/generate', async (req, res) => {
  const [error, query] = validate(req.query, Query, { coerce: true });

  if (error) {
    res.json({ success: false, errors: error.failures() });
  } else {
    const filename = await generateProof(query);
    const proof = await getFile(filename);
    await deleteFile(filename);
    res.setHeader('Content-Disposition', `attachment; filename=${filename}`);
    res.type('application/octet-stream');
    res.send(proof);
  }
});

app.listen(8123, () => console.log('Listening on 8123...'));
