<html>
<body>
POST
<form method="post">
Name: <input type="text" name="name">
Email: <input type="text" name="email">
<input type="submit">
</form>
    <br>
GET
<form method="get">
Name: <input type="text" name="name">
Email: <input type="text" name="email">
<input type="submit">
</form>
    <br>
POST<br>
Welcome <?php if(isset($_POST["name"])) {echo $_POST["name"];} ?>!
Your email address is <?php if(isset($_POST["email"])) {echo $_POST["email"];} ?> 
    <br>
GET<br>
Welcome <?php if(isset($_GET["name"])) { echo $_GET["name"];} ?>!
Your email address is <?php if(isset($_GET["email"])) {echo $_GET["email"]; }?> 

</body>
</html> 