const webpack = require('webpack');
const chalk = require('chalk');
const clear = require('clear');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const DotenvWebpackPlugin = require('dotenv-webpack');
const FriendlyErrorsWebpackPlugin = require('friendly-errors-webpack-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const path = require('path');

module.exports = (params) => {
  clear();
  console.log(chalk.greenBright(`Creating build...\n`));

  const distPath = path.resolve(__dirname, 'dist');
  const publicPath = path.resolve(__dirname, 'public');
  const sourcePath = path.resolve(__dirname, 'src');

  return {
    entry: [sourcePath],
    devtool: 'source-map',
    stats: 'none',

    output: {
      path: distPath,
      filename: 'main.[contenthash].js',
      publicPath: '/',
    },

    module: {
      rules: [
        {
          test: /\.js$/,
          exclude: /(node_modules)/,
          use: ['babel-loader'],
        },
        {
          test: /\.css$/,
          use: ['style-loader', 'css-loader'],
        },
      ],
    },

    resolve: {
      alias: {
        '~': sourcePath,
      },
    },

    devServer: {
      historyApiFallback: true,
      port: process.env.PORT || 3000,
      stats: 'none',
      quiet: true,
    },

    plugins: [
      new webpack.ProvidePlugin({ process: 'process/browser' }),
      new CopyWebpackPlugin([
        {
          from: path.resolve(
            __dirname,
            'node_modules',
            '@tonclient',
            'lib-web',
            'tonclient.wasm'
          ),
        },
      ]),

      new CopyWebpackPlugin([{ from: publicPath, ignore: ['index.html'] }]),
      new DotenvWebpackPlugin({ path: '.env', ignoreStub: true }),
      new FriendlyErrorsWebpackPlugin(),
      new HtmlWebpackPlugin({
        template: path.resolve(publicPath, 'index.html'),
        filename: 'index.html',
      }),
    ],
  };
};
