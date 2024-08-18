# Bank Management System 
This is an interactive Command line based Bank Management System. This is a client-server based application is made in C-Language and uses the principles of Socket Programming in C.

## File Structure
Database folder acts as a database where account details of the users is stored. The folders are inside the database are made based on the username of the users and it contains 2 files:

#### account_details.txt
This file consists of ID, password, account balance and personal details of the user.
#### password.txt
This file stores the password of the user.

#### mapping.txt
This file maps the account to the username in the database. This file is required when the user needs to send funds to other accounts.

#### login_info.txt
This file consists the existing usernames of the users.

## Execution
Spawn 2 terminals. In one of the terminal compile and run the server and in the other compile and run the:

### Bank_Management_System_server.c
Server file
#### Compile and Run
<div style="position: relative; border: 1px solid #ccc; padding: 10px; border-radius: 5px;">
    <!-- <button  style="
        position: absolute; 
        top: 5px; 
        right: 5px; 
        padding: 5px 10px; 
        background-color: #4CAF50; 
        color: white; 
        border: none; 
        border-radius: 5px; 
        cursor: pointer;
        font-size: 14px;">
        Copy
    </button> -->
    <pre><code id="codeBlock">gcc Bank_Management_System_server.c -o ./executables/server
cd executables
./server</code></pre>
</div>



### Bank_Management_System_client.c
Client file
#### Compile and Run
<div style="position: relative; border: 1px solid #ccc; padding: 10px; border-radius: 5px;">
    <!-- <button style="
        position: absolute; 
        top: 5px; 
        right: 5px; 
        padding: 5px 10px; 
        background-color: #4CAF50; 
        color: white; 
        border: none; 
        border-radius: 5px; 
        cursor: pointer;
        font-size: 14px;">
        Copy
    </button> -->
    <pre><code id="codeBlock">gcc Bank_Management_System_client.c -o ./executables/server
cd executables
./client</code></pre>
</div>

