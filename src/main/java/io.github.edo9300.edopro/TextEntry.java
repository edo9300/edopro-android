package io.github.edo9300.edopro;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.InputType;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.widget.EditText;

public class TextEntry extends Activity {
	private static native void putMessageBoxResult(String text);

    private AlertDialog mTextInputDialog;
    private EditText mTextInputWidget;

	@Override
	@SuppressWarnings("unused")
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		int multiLineTextInput = 1;
		int SingleLineTextInput = 2;
		int SingleLinePasswordInput = 3;
		Bundle b = getIntent().getExtras();
		String acceptButton;
		String hint;
		String current;
		int editType;
		if(b==null){
			acceptButton = "acceptButton";
			hint = "hint";
			current = "current";
			editType = 1;
		} else {
			acceptButton = b.getString("acceptButton");
			hint = b.getString("hint");
			current = b.getString("current");
			editType = b.getInt("editType");
		}

		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		mTextInputWidget = new EditText(this);
		mTextInputWidget.setHint(hint);
		mTextInputWidget.setText(current);
		mTextInputWidget.setMinWidth(300);
		if (editType == SingleLinePasswordInput) {
			mTextInputWidget.setInputType(InputType.TYPE_CLASS_TEXT |
					InputType.TYPE_TEXT_VARIATION_PASSWORD);
		} else {
			mTextInputWidget.setInputType(InputType.TYPE_CLASS_TEXT);
		}

		builder.setView(mTextInputWidget);

		if (editType == multiLineTextInput) {
			builder.setPositiveButton(acceptButton, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int whichButton) {
					pushResult(mTextInputWidget.getText().toString());
				}
			});
		}

		builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
			public void onCancel(DialogInterface dialog) {
				cancelDialog();
			}
		});

		mTextInputWidget.setOnKeyListener(new OnKeyListener() {
			@Override
			public boolean onKey(View view, int KeyCode, KeyEvent event) {
				if (KeyCode == KeyEvent.KEYCODE_ENTER) {

					pushResult(mTextInputWidget.getText().toString());
					return true;
				}
				return false;
			}
		});

		mTextInputDialog = builder.create();
		mTextInputDialog.show();
	}

    private void pushResult(String text) {
		putMessageBoxResult(text);
		mTextInputDialog.dismiss();
		finish();
	}

    private void cancelDialog() {
		mTextInputDialog.dismiss();
		finish();
	}
}
