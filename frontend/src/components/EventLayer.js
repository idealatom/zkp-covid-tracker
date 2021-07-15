import React, { useState } from 'react';
import { Circle, Marker, Rectangle, useMapEvents } from 'react-leaflet';

const CLICK_THRESHOLD = 6;

const EventLayer = ({
  target,
  center,
  area,
  radius,
  onClick,
  onCircleDrag,
  onRectDrag,
  circle,
}) => {
  const [circleDragging, setCircleDragging] = useState(false);
  const [rectDragging, setRectDragging] = useState(false);
  const [prevMousePosition, setPrevMousePosition] = useState(null);
  const [isMapMoved, setIsMapMoved] = useState(false);
  const [isMouseDown, setIsMouseDown] = useState(false);
  const map = useMapEvents({
    mousemove: (e) => {
      if (
        isMouseDown &&
        prevMousePosition &&
        e.latlng.distanceTo(prevMousePosition) > CLICK_THRESHOLD
      ) {
        setIsMapMoved(true);
      }

      if (circleDragging) {
        onCircleDrag(e.latlng);
      }

      if (rectDragging) {
        onRectDrag(prevMousePosition, e.latlng);
        setPrevMousePosition(e.latlng);
      }
    },
    mousedown: (e) => {
      setIsMouseDown(true);
      setPrevMousePosition(e.latlng);
    },
    mouseup: (e) => {
      if (!isMapMoved) {
        onClick(e.latlng);
      }

      map.dragging.enable();
      setCircleDragging(false);
      setRectDragging(false);
      setIsMapMoved(false);
      setIsMouseDown(false);
    },
  });
  const circleEventHandlers = {
    mousedown: () => {
      map.dragging.disable();
      setCircleDragging(true);
    },
  };
  const rectEventHandlers = {
    mousedown: () => {
      map.dragging.disable();
      setRectDragging(true);
    },
  };
  return (
    <React.Fragment>
      {/*<Circle
        eventHandlers={circleEventHandlers}
        center={center}
        radius={radius}
        ref={circle}
        />*/}
      <Rectangle eventHandlers={rectEventHandlers} bounds={area} color="red"/>
      {target && <Marker position={target} />}
    </React.Fragment>
  );
};

export default EventLayer;
