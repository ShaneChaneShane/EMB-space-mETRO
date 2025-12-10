const { getRainProtectorStatus } = require("../../utils/blynkService");

module.exports = async (req, res, next) => {
  try {
    // request current rainProtector status from blynk
    const result = await getRainProtectorStatus();
    return res.status(200).json({
      success: true,
      data: {
        isOpen: result.data.isOpen,
      },
    });
  } catch (err) {
    return res.status(500).json({
      success: false,
      message: err.message,
    });
  }
};
