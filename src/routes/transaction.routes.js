import express from 'express';
import signup from '../controllers/signup.controller.js';

const routes = express.Router();

routes.patch('/', signup.transact)

export default routes