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
        humidityEnv: Number(response.data.V2),
        humidityClothes: Number(response.data.V1),
        rain: Number(response.data.V4),
        temperature: Number(response.data.V0),
        light: Number(response.data.V3),
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
    // const getStatusUrl = `https://blynk.cloud/external/api/get?token=${BLYNK_TOKEN}&${BLYNK_RAIN_PROTECTOR_STATUS_PIN}`;
    // const getStatusResponse = await axios.get(getStatusUrl);
    // const isOpen = getStatusResponse.data == "open";
    return {
      success: true,
      data: {
        // isOpen: isOpen,
        // MOCK
        isOpen: false,
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
    // const getProcessingUrl = `https://blynk.cloud/external/api/get?token=${BLYNK_TOKEN}&${BLYNK_MOTOR_PROCESSING_PIN}`;
    // const getProcessingResponse = await axios.get(getProcessingUrl);
    // const isProcessing = Number(getProcessingResponse.data) === 1;
    // if (isProcessing) {
    //   throw new Error("Rain protector is currently processing");
    // }

    // // update
    // const value = shouldOpen ? 1 : 0;
    // const newData = "test from API";
    // const updateUrl = `https://blynk.cloud/external/api/update?token=${BLYNK_TOKEN}&V1=${newData}`;
    // const updateResponse = await axios.post(updateUrl);
    // console.log(updateResponse.data);
    // const updateUrl = `https://blynk.cloud/external/api/update?token=${BLYNK_TOKEN}&${BLYNK_RAIN_PROTECTOR_COMMAND_PIN}=${value}`;
    // const updateResponse = await axios.get(updateUrl);

    const success =
      // updateResponse.data === true || updateResponse.data === "true";
      true; // MOCK

    return {
      success,
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
