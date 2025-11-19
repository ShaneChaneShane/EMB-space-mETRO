// mock data
const getWeatherFromBlynk = require("../../utils/blynkService");
const { RainStatus, SunlightLevel } = require("../../utils/enums");
const evaluateSensorData = require("../../utils/evaluateSensorData");

module.exports = async (req, res, next) => {
  const data = await getWeatherFromBlynk();
  const evaluatedData = await evaluateSensorData(
    data.humidityEnv,
    data.humidityClothes,
    data.rain,
    data.temperature,
    data.light
  );

  // TO-DO: error handling: when blynk data can't be fetched or evaluated
  return res.status(200).json({
    success: true,
    data: evaluatedData,
  });
};
