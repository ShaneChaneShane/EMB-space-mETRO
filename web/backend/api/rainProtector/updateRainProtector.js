const { switchRainProtector } = require("../../utils/blynkService");

module.exports = async (req, res, next) => {
  try {
    const shouldOpen = req.body.shouldOpen;
    const result = await switchRainProtector(shouldOpen);
    if (!result.success) {
      throw new Error(result.message);
    }

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
