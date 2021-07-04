import React, { useState } from "react";
import { MapContainer, TileLayer, Marker, Popup } from "react-leaflet";

function MapBox() {
  const [position, _setPosition] = useState([9.776018, 99.978224]);
  // 9.776018736085613, 99.97822407742707
  // const [lng, setLng] = useState(-70.9);
  // const [lat, setLat] = useState(42.35);
  // const [zoom, setZoom] = useState(9);

  // return 1;

  return (
    <div>
      <MapContainer
        style={{ height: "500px" }}
        center={position}
        zoom={16}
        // scrollWheelZoom={false}
      >
        <TileLayer
          attribution='&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
      </MapContainer>
    </div>
  );
}

export default MapBox;
