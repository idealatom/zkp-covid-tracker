import { signerKeys, TonClient } from '@tonclient/core';
import { libWeb } from '@tonclient/lib-web';
const { Account } = require('@tonclient/appkit');

// const NETWORK = 'http://192.168.1.22';
// const NETWORK = 'http://live.freeton.nil.foundation/';
const NETWORK = 'http://main3.ton.dev';
// const ADDRESS =
// '0:87884999045df1da979e80f4384c1b5c1dbeafb9acde746684c18ea582999987';

const ADDRESS =
  '0:e2bc9bff563553ece575f94d3ae70e885dedfee9fd440bce81c2ae12d78e7821';

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
    const keys = await client.crypto.generate_random_sign_keys();
    console.log('proof', proof);
    console.log('client', client);
    const signer = signerKeys(keys);
    const account = new Account(HelloContract, {
      client,
      signer,
      address: ADDRESS,
    });
    const smth = await account.run('touch');
    console.log('smth', smth);
  });
}

const HelloContract = {
  abi: {
    'ABI version': 2,
    header: ['time', 'expire'],
    functions: [
      {
        name: 'constructor',
        inputs: [],
        outputs: [],
      },
      {
        name: 'renderHelloWorld',
        inputs: [],
        outputs: [{ name: 'value0', type: 'bytes' }],
      },
      {
        name: 'touch',
        inputs: [],
        outputs: [],
      },
      {
        name: 'sendValue',
        inputs: [
          { name: 'dest', type: 'address' },
          { name: 'amount', type: 'uint128' },
          { name: 'bounce', type: 'bool' },
        ],
        outputs: [],
      },
      {
        name: 'timestamp',
        inputs: [],
        outputs: [{ name: 'timestamp', type: 'uint32' }],
      },
    ],
    data: [],
    events: [],
  },
};
