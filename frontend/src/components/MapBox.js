import React, { useCallback, useRef, useState } from 'react';
import * as L from 'leaflet';
import {
  makeStyles,
  TextField,
  Button,
  CircularProgress,
  Typography,
} from '@material-ui/core';
import { MapContainer, TileLayer } from 'react-leaflet';
import { calculateRectArea, download, fillWithRectangles } from '~/utils';
import * as api from '~/api';
import { useStore } from '~/Store';
import {
  addProof,
  setGenerationPending,
  setVerificationPending,
  setVerificationResult,
} from '~/state';
import EventLayer from './EventLayer';
import TestRectangle from './Rectangle';
import * as ton from '~/tonclient';

const latLngToString = (latlng) =>
  L.GeoJSON.latLngToCoords(latlng)
    .reverse()
    .map((n) => n.toFixed(6))
    .join(' ');

const boundsToParams = (bounds) => ({
  minLat: bounds.getWest(),
  minLng: bounds.getNorth(),
  maxLat: bounds.getEast(),
  maxLng: bounds.getSouth(),
});

const useStyles = makeStyles((theme) => ({
  root: {
    display: 'flex',
    flex: '1 1 auto',
    flexDirection: 'row',
  },
  map: {
    width: '100%',
    height: '100vh',
  },
  form: {
    width: '400px',
    padding: `0 ${theme.spacing(1)}px`,
  },
  line: {
    display: 'flex',
    flexDirection: 'row',
  },
  field: {
    width: '100%',
    display: 'flex',
    flex: '1 1 auto',
    margin: `${theme.spacing(1)}px 0`,
  },
  button: {
    width: '100%',
    height: '56px',
    display: 'flex',
    flex: '1 1 auto',
    margin: `${theme.spacing(1)}px 0`,
  },
  title: {
    paddingTop: theme.spacing(1),
    textAlign: 'center',
    textTransform: 'uppercase',
  },
  horizontal: {
    display: 'flex',
    flexDirection: 'row',
    '& > button': {
      maxWidth: '100px',
      marginLeft: theme.spacing(1),
    },
  },
}));

function MapBox() {
  const classes = useStyles();
  const [position, _setPosition] = useState(new L.LatLng(9.776018, 99.978224));
  const [target, setTarget] = useState(null);
  const [center, setCenter] = useState(new L.LatLng(9.776018, 99.978224));
  const [radius, _setRadius] = useState(200);
  const [area, setArea] = useState(
    L.latLngBounds([9.7828, 99.976421], [9.781003, 99.979574])
  );
  const [resolution, setResolution] = useState(20);
  const [rectangles, setRectangles] = useState([]);
  const [state, dispatch] = useStore();
  const circleRef = useRef();

  const getProof = useCallback(async () => {
    dispatch(setGenerationPending(true));
    try {
      const data = await api.proof(target, boundsToParams(area));
      dispatch(setGenerationPending(false));
      dispatch(addProof(data));
    } catch (e) {
      dispatch(setGenerationPending(false));
      throw e;
    }
  }, [target, center, radius]);

  const verifyProof = useCallback(async () => {
    try {
      dispatch(setVerificationResult(null));
      dispatch(setVerificationPending(true));
      const response = await ton.verify(state.lastProof.hex);
      const result = response.decoded.output.value0;
      dispatch(setVerificationPending(false));
      dispatch(setVerificationResult(String(result)));
    } catch (e) {
      dispatch(setVerificationPending(false));
      dispatch(setVerificationResult(null));
    }
  }, [state]);

  //   const getProof = useCallback(() => {
  //     dispatch(setPending());
  //     api.proof(target, center, radius).then((data) => dispatch(addProof(data)));
  //   }, [target, center, radius]);

  return (
    <div className={classes.root}>
      <MapContainer center={position} zoom={16} className={classes.map}>
        <TileLayer
          attribution='&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        <EventLayer
          target={target}
          area={area}
          center={center}
          radius={radius}
          onClick={setTarget}
          onCircleDrag={setCenter}
          onRectDrag={(prev, current) =>
            setArea(calculateRectArea(area, prev, current))
          }
          circle={circleRef}
        />
        {rectangles.map((b) => (
          <TestRectangle bounds={b} key={b.toBBoxString()} />
        ))}
      </MapContainer>
      <div className={classes.form}>
        <Typography className={classes.title} variant="h6">
          Proof
        </Typography>
        <TextField
          className={classes.field}
          variant="outlined"
          value={target ? latLngToString(target) : ''}
        />
        <TextField
          className={classes.field}
          variant="outlined"
          value={center ? latLngToString(center) : ''}
        />
        <Button
          className={classes.button}
          variant="contained"
          color="primary"
          size="large"
          disableElevation
          disabled={!target || state.generationPending}
          onClick={() => getProof()}
        >
          {state.generationPending ? (
            <CircularProgress color="inherit" disableShrink />
          ) : (
            'Generate Proof'
          )}
        </Button>
        {state.lastProof && (
          <React.Fragment>
            <Typography className={classes.title} variant="h6">
              Verification
            </Typography>
            <TextField
              className={classes.field}
              variant="outlined"
              value={state.lastProof.hex}
              multiline
              rows={6}
            />
            <Button
              className={classes.button}
              variant="contained"
              color="primary"
              size="large"
              disableElevation
              disabled={state.generationPending}
              onClick={() => download('proof.hex', state.lastProof.hex)}
            >
              Download
            </Button>
            <Button
              className={classes.button}
              variant="contained"
              color="primary"
              size="large"
              disableElevation
              disabled={state.verificationPending}
              onClick={() => verifyProof()}
            >
              {state.verificationPending ? (
                <CircularProgress color="inherit" disableShrink />
              ) : (
                'Verify'
              )}
            </Button>
            {state.verificationResult !== null && (
              <Typography
                className={classes.title}
                color={
                  state.verificationResult === 'true'
                    ? 'textSecondary'
                    : 'error'
                }
                variant="h6"
              >
                Verification Result: {state.verificationResult}
              </Typography>
            )}
          </React.Fragment>
        )}
        <Typography className={classes.title} variant="h6">
          Test
        </Typography>
        <div className={classes.horizontal}>
          <TextField
            className={classes.field}
            variant="outlined"
            value={resolution}
            onChange={(event) => setResolution(Number(event.target.value))}
          />
          <Button
            className={classes.button}
            variant="contained"
            color="primary"
            size="large"
            disableElevation
            disabled={resolution < 1}
            onClick={() =>
              setRectangles(fillWithRectangles(circleRef.current, resolution))
            }
          >
            Test
          </Button>
        </div>
      </div>
    </div>
  );
}

export default MapBox;
