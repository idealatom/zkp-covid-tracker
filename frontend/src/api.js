export async function proof(pos, area) {
  const query = new URLSearchParams({
    posLat: pos.lat,
    posLng: pos.lng,
    ...area,
  });
  const res = await fetch(`${process.env.BACKEND_URL}/proof/generate?${query}`);
  const json = await res.json();
  console.log('json', json);
  return json;
}
