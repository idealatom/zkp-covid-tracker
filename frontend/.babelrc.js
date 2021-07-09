module.exports = {
  presets: ['@babel/preset-env', '@babel/preset-react'],
  plugins: [
    '@babel/plugin-proposal-nullish-coalescing-operator',
    '@babel/plugin-transform-runtime',
    [
      'import',
      {
        libraryName: 'react-use',
        libraryDirectory: 'lib',
        camel2DashComponentName: false,
      },
    ],
  ],
};
