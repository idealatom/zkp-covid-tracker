import React from 'react';
import { Outlet, useNavigate } from 'react-router-dom';
import {
  AppBar,
  Button,
  makeStyles,
  Toolbar,
  Typography,
} from '@material-ui/core';

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
  // contentContainer: {
  //   display: "flex",
  //   flex: "1 1 auto",
  //   overflow: "hidden",
  // },
  // content: {
  //   flex: "1 1 auto",
  //   height: "100%",
  //   overflow: "auto",
  // },
  toolbar: {
    height: 64,
    color: '#aaaaaa',
  },
}));

const MainLayout = () => {
  const navigate = useNavigate();
  const classes = useStyles();

  return (
    <div className={classes.root}>
      <AppBar elevation={0}>
        <Toolbar className={classes.toolbar}>
          <Typography variant="h6" color="secondary" className={classes.title}>
            TRACKER
          </Typography>
          <Button
            onClick={() => navigate('/proof', { replace: true })}
            color="secondary"
          >
            PROOF
          </Button>
          <Button
            onClick={() => navigate('/verify', { replace: true })}
            color="secondary"
          >
            VERIFY
          </Button>
        </Toolbar>
      </AppBar>
      <div className={classes.wrapper}>
        <Outlet />
      </div>
    </div>
  );
};

export default MainLayout;
