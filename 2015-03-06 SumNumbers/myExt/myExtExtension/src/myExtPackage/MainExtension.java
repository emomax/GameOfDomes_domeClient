package myExtPackage;
import com.smartfoxserver.v2.extensions.SFSExtension;
 
public class MainExtension extends SFSExtension{
    @Override
    public void init()
    {
        addRequestHandler("SumNumbers", UpdateTransform.class);
    }
}