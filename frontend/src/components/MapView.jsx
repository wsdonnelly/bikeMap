// frontend/src/components/MapView.jsx
import React from 'react';
import {
  MapContainer,
  TileLayer,
  Marker,
  Polyline,
  useMapEvents
} from 'react-leaflet';
import L from 'leaflet';

delete L.Icon.Default.prototype._getIconUrl;
L.Icon.Default.mergeOptions({
  iconUrl:   'https://unpkg.com/leaflet@1.9.4/dist/images/marker-icon.png',
  iconRetinaUrl:
    'https://unpkg.com/leaflet@1.9.4/dist/images/marker-icon-2x.png',
  shadowUrl:
    'https://unpkg.com/leaflet@1.9.4/dist/images/marker-shadow.png'
});

function ClickHandler({ onMapClick }) {
  useMapEvents({
    click(e) {
      onMapClick(e.latlng);
    }
  });
  return null;
}

export default function MapView({
  onMapClick,
  snappedStart,
  snappedEnd,
  routeCoords
}) {
  return (
    <MapContainer
      center={[60.1699, 24.9384]}
      zoom={13}
      style={{ height: '100%', width: '100%' }}
      maxBounds={[
        [59.9, 24.5],
        [60.5, 25.5]
      ]}
      minZoom={12}
      maxZoom={18}
    >
      <TileLayer
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        attribution="© OSM contributors"
      />
      <ClickHandler onMapClick={onMapClick} />

      {snappedStart && (
        <Marker position={[snappedStart.lat, snappedStart.lon]} />
      )}
      {snappedEnd && (
        <Marker position={[snappedEnd.lat, snappedEnd.lon]} />
      )}

      {routeCoords.length > 0 && (
        <Polyline positions={routeCoords} color="#007AFF" weight={4} />
      )}
    </MapContainer>
  );
}
