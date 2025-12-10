const express = require("express");
const router = express.Router({ mergeParams: true });

const api = require("../api");

router.get("/", api.weather.getWeatherStatus);

module.exports = router;
