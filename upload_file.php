<?PHP

    $path = "data";
    $path = $path . basename( $_FILES['data']['name']);
    if(move_uploaded_file($_FILES['data']['tmp_name'], $path)) {
      echo "The file ".  basename( $_FILES['data']['name']).
      " has been uploaded";
    } else{
        echo "There was an error uploading the file, please try again!";
    
  }
?>