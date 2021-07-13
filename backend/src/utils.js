import path from 'path';
import fs from 'fs/promises';
import os from 'os';

export async function withTempDir(prefix, fn) {
  const pwd = await fs.mkdtemp(path.join(os.tmpdir(), prefix));
  const cleanup = () => fs.rmdir(pwd, { recursive: true });
  try {
    const result = await fn(pwd);
    // await cleanup();
    return result;
  } catch (e) {
    // await cleanup();
    throw e;
  }
}
