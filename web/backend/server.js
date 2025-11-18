const express = require("express");
const dotenv = require("dotenv");
const helmet = require("helmet");
const { xss } = require("express-xss-sanitizer");
const hpp = require("hpp");
const cors = require("cors");

const app = express();
dotenv.config({ path: "./config/config.env" });

// libraries
app.use(helmet());
app.use(xss());
app.use(hpp());
app.use(cors());

// route files
const weather = require("./routes/weather");
const rainProtector = require("./routes/rainProtector");
const errorHandler = require("./middleware/errorHandler");

// use routes
app.use("/api/weather", weather);
app.use("/api/rainProtector", rainProtector);
app.use(errorHandler);

// configs
const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});

process.on("unhandledRejection", (err, promise) => {
  console.log(`Error: ${err.message}`);
  server.close(() => process.exit(1));
});

module.exports = app;
