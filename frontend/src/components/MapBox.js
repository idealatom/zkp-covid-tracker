import React, { useEffect, useRef, useState } from 'react';
import { GeoJSON } from 'leaflet';
import { makeStyles, TextField, Box } from '@material-ui/core';
import {
  MapContainer,
  TileLayer,
  Marker,
  Popup,
  Circle,
  useMapEvent,
  useMapEvents,
} from 'react-leaflet';

const latLngToCoords = (latlng) => GeoJSON.latLngToCoords(latlng).reverse();
const diff = (a, b) => a.map((n, i) => Math.abs(n - b[i]));
const sum = (xs) => xs.reduce((acc, x) => acc + x, 0);

const CLICK_THRESHOLD = 0.00002;

const useStyles = makeStyles((theme) => ({
  root: {
    display: 'flex',
    flex: '1 1 auto',
    flexDirection: 'row',
    '& .MuiTextField-root': {
      margin: theme.spacing(1),
      width: '100%',
      // width: "25ch",
    },
  },
  map: {
    width: '100%',
    height: '500px',
  },
  form: {
    width: '400px',
  },
  line: {
    display: 'flex',
    flexDirection: 'row',
  },
}));

const EventLayer = ({ target, area, radius, onClick, onCircleDrag }) => {
  const [dragging, setDragging] = useState(false);
  const [prevPosition, setPrevPosition] = useState(null);
  const map = useMapEvents({
    mousemove: (e) => dragging && onCircleDrag(latLngToCoords(e.latlng)),
    mousedown: (e) => setPrevPosition(latLngToCoords(e.latlng)),
    mouseup: (e) =>
      (!prevPosition ||
        sum(diff(prevPosition, latLngToCoords(e.latlng))) < CLICK_THRESHOLD) &&
      onClick(latLngToCoords(e.latlng)),
  });
  const circleEventHandlers = {
    mousedown: () => {
      map.dragging.disable();
      setDragging(true);
    },
    mouseup: () => {
      map.dragging.enable();
      setDragging(false);
    },
  };
  return (
    <React.Fragment>
      <Circle
        eventHandlers={circleEventHandlers}
        center={area}
        radius={radius}
      />
      {target && <Marker position={target} />}
    </React.Fragment>
  );
};

function MapBox() {
  const classes = useStyles();
  const [position, _setPosition] = useState([9.776018, 99.978224]);
  const [target, setTarget] = useState(null);
  const [area, setArea] = useState([9.776018, 99.978224]);
  const [radius, setRadius] = useState(200);

  return (
    <div className={classes.root}>
      <MapContainer
        center={position}
        zoom={16}
        // scrollWheelZoom={false}
        className={classes.map}
      >
        <TileLayer
          attribution='&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        <EventLayer
          target={target}
          area={area}
          radius={radius}
          onClick={setTarget}
          onCircleDrag={setArea}
        />
      </MapContainer>
      <Box my={4} className={classes.form}>
        <div className={classes.line}>
          <TextField
            variant="outlined"
            value={target ? target.map((n) => n.toFixed(6)).join(' ') : ''}
          />
        </div>
        <div className={classes.line}>
          <TextField
            variant="outlined"
            value={position ? position.join(' ') : ''}
          />
        </div>
        <div className={classes.line}>
          <TextField variant="outlined" value={area ? area.join(' ') : ''} />
        </div>
      </Box>
    </div>
  );
}

export default MapBox;
