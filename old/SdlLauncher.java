package io.github.edo9300.edopro;

import android.content.Intent;
import android.os.Bundle;
import org.libsdl.app.SDLActivity;

import java.util.ArrayList;

public class SdlLauncher extends SDLActivity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Bundle ex = getIntent().getExtras();
        ArrayList<String> arg_list = new ArrayList<String>();
        if(ex != null) {
            arg_list = ex.getStringArrayList("ARGUMENTS");
        }
        Object[] array = arg_list.toArray();
        String [] strArr = new String[array.length];
        for(int i = 0 ; i < array.length ; i ++){
            try {
                strArr[i] = array[i].toString();
            } catch (NullPointerException exc) {
                // do some default initialization
            }
        }
        Intent intent = new Intent(this, EpNativeActivity.class);
        intent.putExtra("ARGUMENTS", strArr);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
                "hidapi",
                "SDL2"
        };
    }
}
