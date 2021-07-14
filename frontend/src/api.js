export async function proof(pos, area) {
  const query = new URLSearchParams({
    posLat: pos.lat,
    posLng: pos.lng,
    ...area,
  });
  const res = await fetch(`${process.env.BACKEND_URL}/proof/generate?${query}`);
  if (!res.ok && res.status === 406) {
    throw Error('Blueprint is not satisfied');
  } else {
    const json = await res.json();
    console.log('json', json);
    return json;
  }
}
