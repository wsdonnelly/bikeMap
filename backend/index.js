const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors')

// Attempt to load the native addons (build/Release/…)
let kdSnap, router;
try {
  kdSnap = require('./bindings/build/Release/kd_snap.node');
  router = require('./bindings/build/Release/route.node');
} catch (err) {
  console.warn('Native addons not found, falling back to JS:', err);
  kdSnap = null;
  router = null;
}

const app = express();
app.use(bodyParser.json());
app.use(cors())

//test
// let start = Date.now();
// console.log(kdSnap.findNearest(60.17, 24.94));  // nodeIdx
// console.log('snap time:', Date.now() - start, 'ms');

// console.log(kdSnap.getNode(42));                // { nodeIdx, lat, lon }
// start = Date.now();
// console.log("find path", router.findPath(0, 100));        // array of {nodeIdx,lat,lon}
// console.log('dijstra time:', Date.now() - start, 'ms');

// Snap endpoint
// app.get('/snap', (req, res) => {
//   const lat = parseFloat(req.query.lat);
//   const lon = parseFloat(req.query.lon);
//   if (kdSnap) {
//     const idx = kdSnap.findNearest(lat, lon);
//     const coord = kdSnap.getNode(idx);
//     res.json(coord);
//   } else {
//     res.status(500).json({error: 'No KD addon'});
//   }
// });
// GET /snap?lat=...&lon=... → { nodeIdx, lat, lon }
app.get('/snap', (req, res) => {
  const lat = parseFloat(req.query.lat);
  const lon = parseFloat(req.query.lon);
  if (isNaN(lat) || isNaN(lon)) {
    return res.status(400).json({ error: 'Invalid lat/lon' });
  }
  try {
    const idx = kdSnap.findNearest(lat, lon);
    const coord = kdSnap.getNode(idx);
    //debug
    console.log('snap request →', { lat, lon }, 'nearest idx →', idx, 'coord →', coord);

    return res.json(coord);
  } catch (e) {
    console.error('Snap error:', e);
    return res.status(500).json({ error: e.message });
  }
});


// Route endpoint
// app.get('/route', (req, res) => {
//   const s = parseInt(req.query.startIdx,10);
//   const e = parseInt(req.query.endIdx,10);
//   if (router) {
//     const path = router.findPath(s,e);
//     res.json({path});
//   } else {
//     res.status(500).json({error: 'No routing addon'});
//   }
// });
// GET /route?startIdx=...&endIdx=... → { path: [ { nodeIdx, lat, lon }, … ] }
app.get('/route', (req, res) => {
  const s = parseInt(req.query.startIdx, 10);
  const e = parseInt(req.query.endIdx,   10);
  console.log("requested route start: ", req.query.startIdx, " - end index: ", req.query.endIdx);
  if (isNaN(s) || isNaN(e)) {
    return res.status(400).json({ error: 'Invalid startIdx/endIdx' });
  }
  try {
    const path = router.findPath(s, e);
    console.log("found path", path.size)
    return res.json({ path });
  } catch (ex) {
    console.error('Route error:', ex);
    return res.status(500).json({ error: ex.message });
  }
});

app.listen(3000, ()=> console.log('Server on http://localhost:3000'));
