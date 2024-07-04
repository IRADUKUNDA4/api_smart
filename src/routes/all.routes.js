import express from 'express';

import register from './signup.routes.js'
import transact from "./transaction.routes.js";
import signups from '../controllers/signup.controller.js';

const routes = express.Router();
// routes.post('/test',(req, res) => res.status(200).json({message:"testing route"}));
routes.post('/create-or-update-user', (req, res) => {
   // Your handler logic here
   res.json({ data: "User created or updated" });
 });
//  routes.post('/signup', signups.signupUser);
routes.use('/signup', register);
routes.get('/user', signups.userById);
routes.use('/transact', transact);

// routes.use('/makeAdmin',isAdmin, makeAdmin)


routes.use((req, res) => {
   return res.status(404).json({ message: "page is not found" })
})

export default routes