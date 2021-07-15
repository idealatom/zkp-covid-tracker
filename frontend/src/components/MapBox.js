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
import EventLayer from './EventLayer';
import TestRectangle from './Rectangle';
import * as ton from '~/tonclient';

const cut = (s, n) => (s.length <= n ? s : `${s.slice(0, n)}...`);

const latLngToString = (latlng) =>
  L.GeoJSON.latLngToCoords(latlng)
    .reverse()
    .map((n) => n.toFixed(6))
    .join(' ');

const boundsToParams = (bounds) => {
  const west = bounds.getWest();
  const north = bounds.getNorth();
  const east = bounds.getEast();
  const south = bounds.getSouth();
  return {
    minLng: Math.min(west, east),
    maxLng: Math.max(west, east),
    minLat: Math.min(north, south),
    maxLat: Math.max(north, south),
  };
};

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
  const [position, _setPosition] = useState(new L.LatLng(13.687187, 100.536428));
  const [target, setTarget] = useState(null);
  const [center, setCenter] = useState(new L.LatLng(13.687187, 100.536428));
  const [radius, _setRadius] = useState(200);
  const [provingPending, setProvingPending] = useState(false);
  const [provingError, setProvingError] = useState('');
  const [proof, setProof] = useState('');
  const [verificationResult, setVerificationResult] = useState(null);
  const [verificationPending, setVerificationPending] = useState(false);
  const [verificationError, setVerificationError] = useState('');
  const [area, setArea] = useState(
    L.latLngBounds([13.697777, 100.523192], [13.673677, 100.551189])
  );
  const [resolution, setResolution] = useState(20);
  const [rectangles, setRectangles] = useState([]);
  const circleRef = useRef();

  const getProof = useCallback(async () => {
    setProvingError(null);
    setVerificationError(null);
    setProvingPending(true);
    try {
      setProof('');
      const data = await api.proof(target, boundsToParams(area));
      setProvingPending(false);
      setProof(data.hex);
    } catch (e) {
      setProvingPending(false);
      setProvingError(e.message);
    }
  }, [target, area, center, radius]);

  const verifyProof = useCallback(async () => {
    try {
      setVerificationError(null);
      setVerificationResult(null);
      setVerificationPending(true);
      const response = await ton.verify(proof);
      const result = response.decoded.output.value0;
      setVerificationPending(false);
      setVerificationResult(String(result));
    } catch (e) {
      setVerificationPending(false);
      setVerificationResult(null);
      setVerificationError(e.message);
    }
  }, [proof]);

  return (
    <div className={classes.root}>
      <MapContainer center={position} zoom={12} className={classes.map}>
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
          User coordinates:
        </Typography>
        <TextField
          className={classes.field}
          variant="outlined"
          value={target ? latLngToString(target) : ''}
          placeholder="Select a point on the map"
        />
        {/*<TextField
          className={classes.field}
          variant="outlined"
          value={center ? latLngToString(center) : ''}
        />*/}
        <Button
          className={classes.button}
          variant="contained"
          color="primary"
          size="large"
          disableElevation
          disabled={!target || provingPending}
          onClick={() => getProof()}
        >
          {provingPending ? (
            <CircularProgress color="inherit" disableShrink />
          ) : (
            'Generate Proof'
          )}
        </Button>
        {!!provingError && (
          <Typography className={classes.title} color="error" variant="h6">
            Error: {provingError}
          </Typography>
        )}
        <React.Fragment>
          <Typography className={classes.title} variant="h6">
            Proof (HEX):
          </Typography>
          <TextField
            className={classes.field}
            variant="outlined"
            value={proof}
            onChange={(event) => setProof(event.target.value)}
            multiline
            rows={6}
          />
          <Button
            className={classes.button}
            variant="contained"
            color="primary"
            size="large"
            disableElevation
            disabled={!proof}
            onClick={() => download('proof.hex', proof)}
          >
            Download
          </Button>
          <Button
            className={classes.button}
            variant="contained"
            color="primary"
            size="large"
            disableElevation
            disabled={!proof || verificationPending}
            onClick={() => verifyProof()}
          >
            {verificationPending ? (
              <CircularProgress color="inherit" disableShrink />
            ) : (
              'Verify'
            )}
          </Button>
          {verificationResult !== null && (
            <Typography
              className={classes.title}
              color={verificationResult === 'true' ? 'textSecondary' : 'error'}
              variant="h6"
            >
              Verification Result: {verificationResult}
            </Typography>
          )}
          {verificationResult === null && !!verificationError && (
            <Typography className={classes.title} color="error" variant="h6">
              Error: {cut(verificationError, 60)}
            </Typography>
          )}
        </React.Fragment>
        {/*
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
        */}
      </div>
    </div>
  );
}

export default MapBox;
