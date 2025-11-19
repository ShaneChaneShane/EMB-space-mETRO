const axios = require("axios");

const BLYNK_TOKEN = process.env.BLYNK_TOKEN;

module.exports = async function getWeatherFromBlynk() {
  // FETCH BLYNK DATA
  // const url = `https://blynk.cloud/external/api/get?token=${BLYNK_TOKEN}&V0&V1&V2`;
  // const response = await axios.get(url);

  // TO-DO: error handling
  // TO-DO: look into what the data returned by blynk looks like
  return {
    // humidityEnv: Number(response.data.V0),
    // humidityClothes: Number(response.data.V1),
    // rain: Number(response.data.V2),
    // temperature: Number(response.data.V3),
    // light: Number(response.data.V4),

    // MOCK DATA
    humidityEnv: 20,
    humidityClothes: 100,
    rain: 50,
    temperature: 25,
    light: 300,
  };
};
