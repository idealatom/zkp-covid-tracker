import React from 'react';
import { SVGOverlay } from 'react-leaflet';

function Square({ bounds }) {
  const color = Math.floor(Math.random() * 16777215).toString(16);
  return (
    <SVGOverlay bounds={bounds}>
      <rect
        x="0"
        y="0"
        width="100%"
        height="100%"
        fill={`#${color}`}
        fillOpacity={0.5}
        stroke="red"
        strokeWidth={1}
      />
    </SVGOverlay>
  );
}

export default Square;
