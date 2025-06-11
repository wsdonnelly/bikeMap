// frontend/src/App.jsx
import React, { useState, useEffect } from 'react';
import MapView from './components/MapView';
import Controls from './components/Controls';
import { snapToGraph, getRoute } from './utils/api';

export default function App() {
  // Holds the snapped start/end points { nodeIdx, lat, lon }
  const [snappedStart, setSnappedStart] = useState(null);
  const [snappedEnd,   setSnappedEnd]   = useState(null);

  // Holds the array of [lat, lon] for the route polyline
  const [routeCoords,  setRouteCoords]  = useState([]);

  // Whenever both endpoints are set, fetch the route
  useEffect(() => {
    if (snappedStart && snappedEnd) {
      getRoute(snappedStart.nodeIdx, snappedEnd.nodeIdx)
        .then(path => {
          // Convert to [lat, lon] pairs
          const coords = path.map(p => [p.lat, p.lon]);
          setRouteCoords(coords);
        })
        .catch(err => {
          console.error('Error fetching route:', err);
          alert('Failed to load route');
        });
    }
  }, [snappedStart, snappedEnd]);

  // Handle a click on the map
  const handleMapClick = async ({ lat, lng }) => {
    try {
      const snapped = await snapToGraph(lat, lng);
      console.log('SNAPPED:', snapped);
      if (!snappedStart) {
        setSnappedStart(snapped);
      } else if (!snappedEnd) {
        setSnappedEnd(snapped);
      } else {
        // Both already set: start a new route
        setSnappedStart(snapped);
        setSnappedEnd(null);
        setRouteCoords([]);
      }
    } catch (err) {
      console.error('Snap error:', err);
      alert('Failed to snap to graph');
    }
  };

  // Clear everything
  const handleClear = () => {
    setSnappedStart(null);
    setSnappedEnd(null);
    setRouteCoords([]);
  };

  return (
    <>
      <MapView
        snappedStart={snappedStart}
        snappedEnd={snappedEnd}
        routeCoords={routeCoords}
        onMapClick={handleMapClick}
      />
      <Controls onClear={handleClear} />
    </>
  );
}
