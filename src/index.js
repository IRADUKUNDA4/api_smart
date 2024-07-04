import express from "express";
import bodyParser from "body-parser";
import mongoose from "mongoose";
import dotenv from "dotenv";
import allRoutes from "./routes/all.routes.js";
import cors from "cors";
import cookieParser from "cookie-parser";
import response from "./utils/response.util.js";
import swaggerUi from "swagger-ui-express";
import docs from "./documentation/index.js";
import signup from "./models/signup.models.js";

const app = express();
const corsOpts = {
  origin: "*",

  methods: ["GET", "POST", "DELETE", "PATCH", "PUT"],

  allowedHeaders: ["Content-Type", "Authorization"],
};
app.use(cors(corsOpts));
dotenv.config();
app.use(cookieParser());
app.use(bodyParser.json());

const newUser = new signup({
  email: "newuser@example.com",
  username: "newuser",
  password: "securepassword", // this password has to be hashed moving forward
  role: "user",
  wallet: 0,
});

app.get("/", (req, res) =>
  response.success(
    res,
    200,
    "welcome to the back-end of my project."
  )
);
app.use("/api-docs", swaggerUi.serve, swaggerUi.setup(docs));
app.use(allRoutes);
const port = process.env.PORT;
mongoose.set("strictQuery", true);
mongoose
  .connect(`${process.env.MONGO_URL}`, {
    useNewUrlParser: true,
    useUnifiedTopology: true,
  })
  .then(async () => {
    console.log("connected to the database");
   //  try {
   //    const savedUser = await newUser.save();
   //    console.log("User saved:", savedUser);
   //  } catch (err) {
   //    console.error(err.message);
   //  }
  })
  .catch((err) => {
    console.log("error", err);
  });
app.listen(4000);
console.log(`the server is listening at http://localhost:${4000}`);
