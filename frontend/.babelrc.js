module.exports = {
  presets: ["@babel/preset-react"],
  plugins: [
    [
      "import",
      {
        libraryName: "react-use",
        libraryDirectory: "lib",
        camel2DashComponentName: false,
      },
    ],
  ],
};
