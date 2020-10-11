package io.github.edo9300.edopro;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.text.InputType;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.widget.EditText;

public class TextEntry {


    private AlertDialog mTextInputDialog;
    private EditText mTextInputWidget;

	public TextEntry() {
	}

	public void Show(android.content.Context context, String current) {
		mTextInputWidget = new EditText(context);
		mTextInputWidget.setMinWidth(300);
		mTextInputWidget.setInputType(InputType.TYPE_CLASS_TEXT);

		AlertDialog.Builder builder = new AlertDialog.Builder(context);
		mTextInputWidget.setText(current);

		builder.setView(mTextInputWidget);

		builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
			public void onCancel(DialogInterface dialog) {
				pushResult(mTextInputWidget.getText().toString(), false);
			}
		});

		mTextInputWidget.setOnKeyListener(new OnKeyListener() {
			@Override
			public boolean onKey(View view, int KeyCode, KeyEvent event) {
				if (KeyCode == KeyEvent.KEYCODE_ENTER) {
					pushResult(mTextInputWidget.getText().toString(), true);
					return true;
				}
				return false;
			}
		});

		mTextInputDialog = builder.create();
		mTextInputDialog.show();
	}

    private void pushResult(String text, boolean isenter) {
		EpNativeActivity.putMessageBoxResult(text, isenter);
		mTextInputDialog.dismiss();
	}
}
