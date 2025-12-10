// mock data
const getWeatherFromBlynk = require("../../utils/blynkService");
const { RainStatus, SunlightLevel } = require("../../utils/enums");
const evaluateSensorData = require("../../utils/evaluateSensorData");

module.exports = async (req, res, next) => {
  try {
    const result = await getWeatherFromBlynk();
    if (!result.success) {
      throw new Error(result.message);
    }
    const data = result.data;
    const evaluatedData = evaluateSensorData(
      data.humidityEnv,
      data.humidityClothes,
      data.rain,
      data.temperature,
      data.light
    );

    return res.status(200).json({
      success: true,
      data: evaluatedData,
    });
  } catch (err) {
    return res.status(500).json({
      success: false,
      message: err.message,
    });
  }
};
