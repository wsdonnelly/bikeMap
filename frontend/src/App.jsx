import React, { useState, useEffect } from 'react'
import { MapView, GraphOverlay } from './components/MapView'
import Controls from './components/Controls'
import { snapToGraph, getRoute, getGraph } from './utils/api'

const App = () => {
  // Holds the snapped start/end points { nodeIdx, lat, lon }
  const [snappedStart, setSnappedStart] = useState(null)
  const [snappedEnd,   setSnappedEnd]   = useState(null)

  // Holds the array of [lat, lon] for the route polyline
  const [routeCoords,  setRouteCoords]  = useState([])

  //for test overlay
  // const [segments, setSegments] = useState([])

// useEffect(() => {
//   getGraph()
//     .then(segments => {
//       console.log('fetched segments (type):', typeof(segments));
//       setSegments(segments);
//     })
//     .catch(err => {
//       console.error('Failed to load graph:', err);
//     });
// }, []);


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

  // Log whenever snappedStart changes
  useEffect(() => {
    console.log('snappedStart updated:', snappedStart);
  }, [snappedStart]);

  // Log whenever snappedEnd changes
  useEffect(() => {
    console.log('snappedEnd updated:', snappedEnd);
  }, [snappedEnd]);

  //   useEffect(() => {
  //   console.log('segmants updated:', typeof(segments));
  // }, [segments]);

  const handleMapClick = async ({ lat, lng }) => {
    try {
      const snapped = await snapToGraph(lat, lng)
      console.log('snapped:', snapped)
      if (!snappedStart)
        setSnappedStart(snapped)
      else if (!snappedEnd)
        setSnappedEnd(snapped)
      else {
        // Both already set: start a new route
        setSnappedStart(snapped)
        setSnappedEnd(null)
        setRouteCoords([])
      }
    } catch (err) {
      console.error('Snap error:', err)
      alert('Failed to snap to graph')
    }
  }

  // Clear everything
  const handleClear = () => {
    setSnappedStart(null);
    setSnappedEnd(null);
    setRouteCoords([]);
  }
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
  )
  // return (
  //   <>
  //     <MapView
  //       snappedStart={snappedStart}
  //       snappedEnd={snappedEnd}
  //       routeCoords={routeCoords}
  //       onMapClick={handleMapClick}
  //     >
  //       <GraphOverlay segments={segments} />
  //     </MapView>

  //     <Controls onClear={handleClear} />
  //   </>
  // )
}

export default App