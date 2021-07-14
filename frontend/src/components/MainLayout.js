import React from 'react';
import { AppBar, makeStyles, Toolbar, Typography } from '@material-ui/core';

const useStyles = makeStyles((theme) => ({
  root: {
    backgroundColor: theme.palette.background.default,
    display: 'flex',
    height: '100%',
    overflow: 'hidden',
    width: '100%',
  },
  title: {
    flexGrow: 1,
  },
  wrapper: {
    display: 'flex',
    flex: '1 1 auto',
    overflow: 'hidden',
    paddingTop: 64,
  },
  toolbar: {
    height: 64,
    color: '#aaaaaa',
  },
}));

const MainLayout = ({ children }) => {
  const classes = useStyles();

  return (
    <div className={classes.root}>
      <AppBar elevation={0}>
        <Toolbar className={classes.toolbar}>
          <Typography variant="h6" color="secondary" className={classes.title}>
            TRACKER
          </Typography>
        </Toolbar>
      </AppBar>
      <div className={classes.wrapper}>{children}</div>
    </div>
  );
};

export default MainLayout;
