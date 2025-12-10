const axios = require("axios");

const BLYNK_TOKEN = process.env.BLYNK_TOKEN;
const RAINPR_PIN = process.env.BLYNK_RAIN_PROTECTOR_PIN;

module.exports = async function getWeatherFromBlynk() {
  try {
    // FETCH BLYNK DATA
    const url = `https://blynk.cloud/external/api/get?token=${BLYNK_TOKEN}&V0&V1&V2&V3&V4`;
    const response = await axios.get(url);
    console.log(response.data);
    return {
      success: true,
      data: {
        temperature: Number(response.data.V0),
        humidityClothes: Number(response.data.V1),
        humidityEnv: Number(response.data.V2),
        light: Number(response.data.V3),
        rain: Number(response.data.V4)==1?0:1,
        // MOCK DATA
        // humidityEnv: 20,
        // humidityClothes: 100,
        // rain: 50,
        // temperature: 25,
        // light: 300,
      },
    };
  } catch (err) {
    return {
      success: false,
      message: err.message,
    };
  }
};

module.exports.getRainProtectorStatus = async function () {
  try {
    // FETCH BLYNK DATA
    const getStatusUrl = `https://blynk.cloud/external/api/get?token=${BLYNK_TOKEN}&V8`;
    const getStatusResponse = await axios.get(getStatusUrl);
    const isOpen = getStatusResponse.data == 1 ? true : false;
    return {
      success: true,
      data: {
        isOpen: isOpen,
      },
    };
  } catch (err) {
    return {
      success: false,
      message: err.message,
    };
  }
};

module.exports.switchRainProtector = async function (shouldOpen) {
  try {
    // FETCH BLYNK DATA
    // check if motor is processing
    const getProcessingUrl = `https://blynk.cloud/external/api/get?token=${BLYNK_TOKEN}&V5`;
    const getProcessingResponse = await axios.get(getProcessingUrl);
    const isProcessing = Number(getProcessingResponse.data) === 1;
    if (isProcessing) {
      throw new Error("Rain protector is currently processing");
    }

    // update
    const value = shouldOpen ? 0 : 1;
    const updateUrl = `https://blynk.cloud/external/api/update?token=${BLYNK_TOKEN}&V6=${value}`;
    const updateResponse = await axios.get(updateUrl);

    return {
      success: true,
      data: {
        isOpen: shouldOpen,
      },
    };
  } catch (err) {
    return {
      success: false,
      message: err.message,
    };
  }
};
