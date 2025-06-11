// frontend/src/components/Controls.jsx
import React from 'react';

export default function Controls({ onClear }) {
  return (
    <div style={styles.container}>
      <button onClick={onClear} style={styles.btn}>
        Clear
      </button>
    </div>
  );
}

const styles = {
  container: {
    position: 'absolute',
    top: 10,
    right: 10,
    zIndex: 1000,
  },
  btn: {
    padding: '8px 12px',
    cursor: 'pointer',
  },
};
