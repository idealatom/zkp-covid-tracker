import * as R from 'ramda';

const mergeBounds = R.reduce((acc, b) => acc.extend(b), {
  extend: R.identity,
});

const mergeCols = R.pipe(
  R.groupBy((b) => b.getWest()),
  R.values,
  R.map(mergeBounds)
);

const mergeRows = R.pipe(
  R.groupBy((b) => b.getNorth()),
  R.values,
  R.map(mergeBounds)
);

const isInRadius = (circle) => (square) =>
  circle.getLatLng().distanceTo(square.getCenter()) < circle.getRadius();

// prettier-ignore
const mergeSquares = (circle, squares) => R.pipe(
  R.filter(isInRadius(circle)),
  mergeCols,
  mergeRows
)(squares);

const pixelize = (circle, resolution) => {
  const bounds = circle.getBounds();
  const left = bounds.getWest();
  const top = bounds.getNorth();
  const width = bounds.getEast() - bounds.getWest();
  const height = bounds.getNorth() - bounds.getSouth();
  const sw = width / resolution;
  const sh = height / resolution;

  const coords = R.xprod(
    R.range(1, resolution + 1),
    R.range(1, resolution + 1)
  );

  const squares = coords.map(([x, y]) =>
    L.latLngBounds(
      [top - sh * y, left + sw * (x - 1)],
      [top - sh * (y - 1), left + sw * x]
    )
  );

  return squares;
};

export function fillWithRectangles(circle, resolution) {
  const squares = pixelize(circle, resolution);
  const merged = mergeSquares(circle, squares);
  return merged;
}

export function download(filename, text) {
  var element = document.createElement('a');
  element.setAttribute(
    'href',
    'data:text/plain;charset=utf-8,' + encodeURIComponent(text)
  );
  element.setAttribute('download', filename);
  element.click();
}

const latLngUtils = {
  sub: (a, b) => L.latLng(a.lat - b.lat, a.lng - b.lng),
  add: (a, b) => L.latLng(a.lat + b.lat, a.lng + b.lng),
  shiftBounds: (bounds, offset) =>
    L.latLngBounds(
      latLngUtils.add(bounds.getSouthWest(), offset),
      latLngUtils.add(bounds.getNorthEast(), offset)
    ),
};

export function calculateRectArea(area, prev, current) {
  const offset = latLngUtils.sub(current, prev);
  const newArea = latLngUtils.shiftBounds(area, offset);
  return newArea;
}
