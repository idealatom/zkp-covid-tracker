export const ADD_PROOF = 'ADD_PROOF';
export const SET_GENERATION_PENDING = 'SET_GENERATION_PENDING';
export const SET_VERIFICATION_PENDING = 'SET_VERIFICATION_PENDING';
export const SET_VERIFICATION_RESULT = 'SET_VERIFICATION_RESULT';

export const initialState = {
  proofs: [],
  lastProof: null,
  generationPending: false,
  verificationPending: false,
  verificationResult: null,
};

export const addProof = (proof) => ({
  type: ADD_PROOF,
  payload: proof,
});

export const setGenerationPending = (state) => ({
  type: SET_GENERATION_PENDING,
  payload: state,
});

export const setVerificationPending = (state) => ({
  type: SET_VERIFICATION_PENDING,
  payload: state,
});

export const setVerificationResult = (state) => ({
  type: SET_VERIFICATION_RESULT,
  payload: state,
});

export const proofListReducer = (state = initialState, action) => {
  switch (action.type) {
    case ADD_PROOF:
      return {
        ...state,
        proofs: action.payload
          ? [...new Set(state.proofs.concat(action.payload))]
          : state.proofs,
        lastProof: action.payload,
      };
    case SET_GENERATION_PENDING:
      return { ...state, generationPending: action.payload };
    case SET_VERIFICATION_PENDING:
      return { ...state, verificationPending: action.payload };
    case SET_VERIFICATION_RESULT:
      return { ...state, verificationResult: action.payload };
    default:
      return state;
  }
};
