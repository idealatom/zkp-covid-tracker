import React from "react";
import { useRoutes } from "react-router-dom";
import { ThemeProvider } from "@material-ui/styles";
import theme from "~/theme";
import routes from "~/routes";
import useStyles from "./styles";

function App() {
  useStyles();
  const routing = useRoutes(routes());
  return <ThemeProvider theme={theme}>{routing}</ThemeProvider>;
}

export default App;
