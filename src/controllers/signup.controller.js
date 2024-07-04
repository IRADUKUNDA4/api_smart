import signup from "../models/signup.models.js";
import bcrypt from "bcrypt";
import response from "../utils/response.util.js";

class signupControllers {
  static async signupUser(req, res) {
    try {
      // const { email, username, password } = req.body;
      const { username } = req.query;
      const salt = bcrypt.genSaltSync(10);
      const passwordHashed = bcrypt.hashSync("password", salt);
      // const check = await signup.findOne({ username });
      // if (check !== null) {
      //   return response.error(res, 400, "username already exist");
      // }
      const newUser = new signup({ email : "pubtoiletmanage@gmail.com", username , password: passwordHashed });
      await newUser.save();
      response.success(res, 201, "signup complete", newUser);
    } catch (error) {
      return response.error(res, 400, "internal server error");
    }
  }

  static async transactAmount(req, res) {
    try {
      // const { email, username, password } = req.body;
      const { username, amount } = req.query;
      const salt = bcrypt.genSaltSync(10);
      const passwordHashed = bcrypt.hashSync("password", salt);
      const newUser = new signup({ email : "pubtoiletmanage@gmail.com", username , password: passwordHashed, wallet: -amount});
      await newUser.save();
      console.log(newUser);
      response.success(res, 201, "signup complete", newUser);
    } catch (error) {
      return response.error(res, 400, "internal server error");
    }
  }

  // 192.168.1.51
  
  static async allUsers(req, res) {

  //   function sumWallets(items) {
  //     const result = [];
  //     const groupedItems = {};
  //     items.forEach(item => {
  //         const username = item.username;
  //         if (!groupedItems[username]) {
  //             groupedItems[username] = {...item}; 
  //             groupedItems[username].wallet += item.wallet; 
  //         } else {
  //             groupedItems[username].wallet += item.wallet; 
  //         }
  //     });
  
  //     Object.values(groupedItems).forEach(item => {
  //         result.push(item);
  //     });
  
  //     return result;
  // }
    
    try {
      const allUsers = await signup.find();
      // const plainObjects = allUsers.map(doc => doc.toObject());
      // const walletSum = sumWallets(plainObjects);
      response.success(res, 200, `all users are : ${allUsers.length}`, allUsers);
    } catch (error) {
      console.log(error);
      return response.error(res, 500, "internal server error");
    }
  }

  static async userById(req, res) {
  //   function sumWallets(items) {
  //     const result = [];
  //     const groupedItems = {};
  //     items.forEach(item => {
  //         const username = item.username;
  //         if (!groupedItems[username]) {
  //             groupedItems[username] = {...item}; 
  //             groupedItems[username].wallet += item.wallet; 
  //         } else {
  //             groupedItems[username].wallet += item.wallet; 
  //         }
  //     });
  
  //     Object.values(groupedItems).forEach(item => {
  //         result.push(item);
  //     });
  //     return result;
  // }
    try {
      const { id } = req.query;
      const check = await signup.findOne({ username : id});
           console.log("card id : ",id);
      if (check == null) {
        return response.error(res, 400, "username does not exist");
      }
      // const allUsers = await signup.find();
      // const plainObjects = allUsers.map(doc => doc.toObject());
      // const walletSum = sumWallets(plainObjects);
      // const user = walletSum.filter((item) => item.username === id);
      // console.log(user[0])
      response.success(res, 200, `Aggregated users are ${check.length}`, check);
    } catch (error) {
      console.log(error);
      return response.error(res, 500, "internal server error");
    }
  }

  static async transact(req, res) {
    try {
      const { amount, op, id } = req.query;
      console.log(amount, op, id)
      // there is no need to check if the user exist because the user must exist before the wallet can be updated(all cards are in the db for now)
      console.log("checking if the user exists");
      // const check = await signup.findOne({ username : id});
      // console.log("check : ",check);
      // if (check == null) {
      //   return response.error(res, 404, "the user does not exist");
      // }
      if (op === "add") {
        const updated =  await signup.updateOne(
          { username: id },
          { $inc: { wallet: amount } }
        );
        response.success(res, 200, updated);
      } else if (op === "sub") {
        await signup.updateOne(
          { username: "Admin" },
          { $inc: { wallet: amount } }
        );
       const updated =  await signup.updateOne(
          { username: id },
          { $inc: { wallet: -amount } }
        );
        console.log(updated);
        response.success(res, 200, updated);
      }

    } catch (error) {
      console.log(error);
      return response.error(res, 500, "internal server error");
    }
  }
}

export default signupControllers;