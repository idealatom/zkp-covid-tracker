const chalk = require("chalk");
const clear = require("clear");
const CopyWebpackPlugin = require("copy-webpack-plugin");
const DotenvWebpackPlugin = require("dotenv-webpack");
const FriendlyErrorsWebpackPlugin = require("friendly-errors-webpack-plugin");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const path = require("path");

function checkEnvironment(env) {
  if (env !== "development" && env !== "production") {
    console.error(
      "Cannot detect your environment. Possible values are: development, production."
    );
    process.exit(1);
  }
}

module.exports = (env) => {
  console.log(env);
  checkEnvironment(env);
  clear();
  console.log(chalk.greenBright(`Creating ${env} build...\n`));

  const distPath = path.resolve(__dirname, "dist");
  const environmentPath = path.resolve(__dirname, `.env.${env}`);
  const publicPath = path.resolve(__dirname, "public");
  const sourcePath = path.resolve(__dirname, "src");

  return {
    entry: [sourcePath],
    node: { fs: "empty", child_process: "empty" },
    devtool: "source-map",
    stats: "none",

    output: {
      path: distPath,
      filename: "main.[contenthash].js",
      publicPath: "/",
    },

    module: {
      rules: [
        {
          test: /\.js$/,
          exclude: /(node_modules)/,
          use: ["babel-loader"],
        },
        {
          test: /\.css$/,
          use: ["style-loader", "css-loader"],
        },
      ],
    },

    resolve: {
      alias: {
        "~": sourcePath,
      },
    },

    devServer: {
      historyApiFallback: true,
      // open: true,
      port: process.env.PORT || 3000,
      stats: "none",
      quiet: true,
    },

    plugins: [
      new CopyWebpackPlugin([{ from: publicPath, ignore: ["index.html"] }]),
      new DotenvWebpackPlugin({ path: environmentPath }),
      new FriendlyErrorsWebpackPlugin(),
      new HtmlWebpackPlugin({
        template: path.resolve(publicPath, "index.html"),
        filename: "index.html",
      }),
    ],
  };
};
