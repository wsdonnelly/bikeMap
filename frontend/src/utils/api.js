// frontend/src/utils/api.js
// export async function snapToGraph(lat, lon) {
//   const res = await fetch(`/snap?lat=${lat}&lon=${lon}`);
//   if (!res.ok) throw new Error('Snap failed');
//   return res.json(); // { nodeIdx, lat, lon }
// }

// export async function getRoute(startIdx, endIdx) {
//   const res = await fetch(`/route?startIdx=${startIdx}&endIdx=${endIdx}`);
//   if (!res.ok) throw new Error('Route failed');
//   const { path } = await res.json();
//   return path; // [ { nodeIdx, lat, lon } … ]
// }

import axios from 'axios';

const API  = axios.create({ baseURL: 'http://localhost:3000' });

export function snapToGraph(lat, lon) {
  return API
    .get('/snap', { params: { lat, lon } })
    .then(res => res.data);
}

export function getRoute(startIdx, endIdx) {
  return API
    .get('/route', { params: { startIdx, endIdx } })
    .then(res => res.data.path);
}

