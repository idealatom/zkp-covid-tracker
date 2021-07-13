import { signerKeys, TonClient } from '@tonclient/core';
import { libWeb } from '@tonclient/lib-web';

const { Account } = require('@tonclient/appkit');

const NETWORK = process.env.NETWORK;
const ADDRESS = process.env.ADDRESS;

console.log('NETWORK', NETWORK);
console.log('ADDRESS', ADDRESS);

const TEST_KEYS = {
  public: 'cf8e62e1ae3e742be97f3f5149cb5a3256706aa457fa1147c5eef4107887b4be',
  secret: 'f8705d45332950cc07765a638390dc3a11784ce7409efa78c930f0738b8d491e',
};

TonClient.useBinaryLibrary(libWeb);

export async function withTonClient(network, fn) {
  const client = new TonClient({
    network: {
      server_address: network,
    },
  });
  try {
    const result = await fn(client);
    client.close();
    return result;
  } catch (e) {
    client.close();
    throw e;
  }
}

export function verify(proof) {
  return withTonClient(NETWORK, async (client) => {
    // const keys = await client.crypto.generate_random_sign_keys();
    const keys = TEST_KEYS;
    console.log('proof', proof);
    console.log('client', client);
    const signer = signerKeys(keys);
    const account = new Account(VerifyContract, {
      client,
      signer,
      address: ADDRESS,
    });
    const result = await account.run('verify', { proof });
    console.log('Result:', result);
    return result;
  });
}

const VerifyContract = {
  abi: {
    'ABI version': 2,
    header: ['pubkey', 'time'],
    functions: [
      {
        name: 'verify',
        inputs: [{ name: 'proof', type: 'bytes' }],
        outputs: [{ name: 'value0', type: 'bool' }],
      },
      {
        name: 'setVerificationKey',
        inputs: [{ name: 'vkey', type: 'bytes' }],
        outputs: [],
      },
      {
        name: 'constructor',
        inputs: [],
        outputs: [],
      },
      {
        name: 'm_vkey',
        inputs: [],
        outputs: [{ name: 'm_vkey', type: 'bytes' }],
      },
    ],
    data: [],
    events: [],
  },
};
