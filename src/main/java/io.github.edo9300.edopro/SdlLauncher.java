package io.github.edo9300.edopro;

import android.content.Intent;
import android.os.Bundle;
import org.libsdl.app.SDLActivity;

public class SdlLauncher extends SDLActivity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent = new Intent(this, EpNativeActivity.class);
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
