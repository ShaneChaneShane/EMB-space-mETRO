const express = require("express");
const router = express.Router({ mergeParams: true });

const api = require("../api/rainProtector");

router.get("/status", api.getRainProtectorStatus);
router.put("/", api.updateRainProtector);

module.exports = router;
