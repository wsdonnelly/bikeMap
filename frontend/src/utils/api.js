import axios from 'axios';

const API  = axios.create({ baseURL: 'http://localhost:3000' })

export function snapToGraph(lat, lon) {
  return API
    .get('/snap', { params: { lat, lon } })
    .then(res => res.data)
}

export function getRoute(startIdx, endIdx) {
  return API
    .get('/route', { params: { startIdx, endIdx } })
    .then(res => res.data.path)
}

export function getGraph() {
  return API
    .get('/all',{})
    .then(res => res.data.segments)
}
