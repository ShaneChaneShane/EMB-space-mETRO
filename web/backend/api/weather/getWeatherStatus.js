// mock data
const { RainStatus, SunlightLevel } = require("../../utils/enums");

module.exports = async (req, res, next) => {
  res.json({
    humidity: 78,
    rain: RainStatus.RAINING,
    sun: SunlightLevel.MODERATE,
    temperature: 29,
    dryness: 3,
  });
};
