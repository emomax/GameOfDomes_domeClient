package myExtPackage;
import com.smartfoxserver.v2.entities.User;
import com.smartfoxserver.v2.entities.data.ISFSObject;
import com.smartfoxserver.v2.entities.data.SFSObject;
import com.smartfoxserver.v2.extensions.BaseClientRequestHandler;
 
public class UpdateTransform extends BaseClientRequestHandler{
    @Override
    public void handleClientRequest(User user, ISFSObject objIn)
    {
        int numA = objIn.getInt("NumA");
        int numB = objIn.getInt("NumB");
 
        ISFSObject objOut = new SFSObject();
        objOut.putInt("NumC", numA + numB);
 
        send("SumNumbers", objOut, user);
    }
}